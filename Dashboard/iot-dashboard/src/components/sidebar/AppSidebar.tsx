"use client"

import * as React from "react"
import {
    AudioWaveform,
    Bot,
    Command,
    GalleryVerticalEnd,
    Microchip,
    Bell,
    CloudDownload
} from "lucide-react"

import {
    Sidebar,
    SidebarContent,
    SidebarFooter,
    SidebarHeader,
    SidebarRail,
} from "@/components/ui/sidebar"
import { NavMain } from "./NavMain"
import { NavProjects } from "./NavProject"
import { NavUser } from "./NavUser"
import { TeamSwitcher } from "./TeamSwitcher"
import { DeviceModal } from "../modals/DeviceModal"
import { DeviceDrawer } from "../drawers/DeviceDrawer"
import { NotificationModal } from "../modals/NotificationModal"
import { useMultiTelemetry } from "../context/MultiTelemetryProvider"
import { addNotification, getRanges } from "@/actions"
import { toast } from "@/hooks/use-toast"
import { OTAList } from "../modals/OTAList"
import { NotificationDrawer } from "../drawers/NotificationDrawer"

// This is sample data.
const data = {
    user: {
        name: "shadcn",
        email: "m@example.com",
        avatar: "/avatars/shadcn.jpg",
    },
    teams: [
        {
            name: "Acme Inc",
            logo: GalleryVerticalEnd,
            plan: "Enterprise",
        },
        {
            name: "Acme Corp.",
            logo: AudioWaveform,
            plan: "Startup",
        },
        {
            name: "Evil Corp.",
            logo: Command,
            plan: "Free",
        },
    ],
    navMain: [
        {
            title: "Devices",
            url: "#",
            icon: Microchip,
            items: [
                {
                    title: "Add new device",
                    url: "#",
                    component: <>
                        <div className="hidden md:block">
                            <DeviceModal />
                        </div>
                        <div className="md:hidden">
                            <DeviceDrawer />
                        </div>
                    </>
                },
            ],
        },
        {
            title: "Notifications",
            url: "#",
            icon: Bell,
            items: [
                {
                    title: "Notifications center",
                    url: "#",
                    component: <>
                        <div className="hidden md:block">
                            <NotificationModal />
                        </div>
                        <div className="md:hidden">
                            <NotificationDrawer />
                        </div>
                    </>
                },
            ],
        },
        {
            title: "OTA Update",
            url: "#",
            icon: CloudDownload,
            items: [
                {
                    title: "Software Update",
                    url: '#',
                    component: <OTAList />
                },
            ],
        },
        {
            title: "Chatbot",
            url: "#",
            icon: Bot,
            items: [
                {
                    title: "General",
                    url: "#",
                },
                {
                    title: "Team",
                    url: "#",
                },
                {
                    title: "Billing",
                    url: "#",
                },
                {
                    title: "Limits",
                    url: "#",
                },
            ],
        },
    ],
}

const fireAlert = async (value: number, minValue: number, maxValue: number, deviceName: string, deviceId: string, sensor: string) => {
    const data: any = {
        deviceId,
        sensor,
        value,
        deviceName
    }
    if (value < minValue) {
        data['minValue'] = minValue
        data['condition'] = 'lower'
        data['content'] = `Your predicted_${sensor} value is less than min value (${value} < ${minValue})`
        await addNotification(data)
        toast({
            variant: 'destructive',
            title: `⚠️Uh oh! predicted_${sensor} value is out of range on ${deviceName}`,
            description: `Your predicted_${sensor} value is less than min value (${value} < ${minValue})`,
        })
    }
    else if (value > maxValue) {
        data['maxValue'] = maxValue
        data['condition'] = 'higher'
        data['content'] = `Your predicted_${sensor} value is more than max value (${value} > ${maxValue})`
        await addNotification(data)
        toast({
            variant: 'destructive',
            title: `⚠️ predicted_${sensor} value is out of range on ${deviceName}`,
            description: `Your predicted_${sensor} value is more than max value (${value} > ${maxValue})`,
        })
    }
}

export function AppSidebar({ ...props }: React.ComponentProps<typeof Sidebar>) {

    // const telemetry = useMultiTelemetry()


    // React.useEffect(() => {
    //     let cancelled = false

    //     async function checkAlerts() {
    //         // 2) Fetch 4 bộ range song song
    //         const [phRes, orpRes, turRes, ecRes] = await Promise.all([
    //             getRanges('pH'),
    //             getRanges('ORP'),
    //             getRanges('TUR'),
    //             getRanges('EC'),
    //         ])
    //         if (cancelled) return

    //         const rangesPH = phRes.data   // Array<{ deviceId, sensor, minValue, maxValue }>
    //         const rangesORP = orpRes.data
    //         const rangesTUR = turRes.data
    //         const rangesEC = ecRes.data

    //         // 3) Duyệt từng device trong telemetry
    //         telemetry.forEach((dev) => {
    //             const id = dev.deviceId
    //             const name = dev.deviceName
    //             // helper lấy value cuối cùng
    //             const last = (arr: number[]) =>
    //                 arr.length ? arr[arr.length - 1] : undefined

    //             // ===== pH =====
    //             const vPH = last(dev.pH)
    //             if (vPH !== undefined) {
    //                 const r = rangesPH?.find((x) => x.deviceId === id)
    //                 if (r) {
    //                     // if (vPH < r.minValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️Uh oh! pH value is out of range on ${name}`,
    //                     //         description: `Your pH value is less than min value (${vPH} < ${r.minValue})`,
    //                     //     })
    //                     // } else if (vPH > r.maxValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ pH value is out of range on ${name}`,
    //                     //         description: `Your pH value is more than min value (${vPH} > ${r.maxValue})`,
    //                     //     })
    //                     // }
    //                     fireAlert(vPH, r.minValue, r.maxValue, name, id, 'pH')
    //                 }
    //             }

    //             // ===== ORP =====
    //             const vORP = last(dev.ORP)
    //             if (vORP !== undefined) {
    //                 const r = rangesORP?.find((x) => x.deviceId === id)
    //                 if (r) {
    //                     // if (vORP < r.minValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ ORP quá thấp trên ${name}`,
    //                     //         description: `${vORP} < ${r.minValue}`,
    //                     //     })
    //                     // } else if (vORP > r.maxValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ ORP quá cao trên ${name}`,
    //                     //         description: `${vORP} > ${r.maxValue}`,
    //                     //     })
    //                     // }
    //                     fireAlert(vORP, r.minValue, r.maxValue, name, id, 'ORP')
    //                 }
    //             }

    //             // ===== TUR =====
    //             const vTUR = last(dev.TUR)
    //             if (vTUR !== undefined) {
    //                 const r = rangesTUR?.find((x) => x.deviceId === id)
    //                 if (r) {
    //                     // if (vTUR < r.minValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ TUR quá thấp trên ${name}`,
    //                     //         description: `${vTUR} < ${r.minValue}`,
    //                     //     })
    //                     // } else if (vTUR > r.maxValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ TUR quá cao trên ${name}`,
    //                     //         description: `${vTUR} > ${r.maxValue}`,
    //                     //     })
    //                     // }
    //                     fireAlert(vTUR, r.minValue, r.maxValue, name, id, 'TUR')
    //                 }
    //             }

    //             // ===== EC =====
    //             const vEC = last(dev.EC)
    //             if (vEC !== undefined) {
    //                 const r = rangesEC?.find((x) => x.deviceId === id)
    //                 if (r) {
    //                     // if (vEC < r.minValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ EC quá thấp trên ${name}`,
    //                     //         description: `${vEC} < ${r.minValue}`,
    //                     //     })
    //                     // } else if (vEC > r.maxValue) {
    //                     //     toast({
    //                     //         variant: 'destructive',
    //                     //         title: `⚠️ EC quá cao trên ${name}`,
    //                     //         description: `${vEC} > ${r.maxValue}`,
    //                     //     })
    //                     // }
    //                     fireAlert(vEC, r.minValue, r.maxValue, name, id, 'EC')
    //                 }
    //             }
    //         })
    //     }

    //     checkAlerts()
    //     return () => { cancelled = true }
    // }, [telemetry.map(d => d.deviceId).join(',')])

    const telemetry = useMultiTelemetry()
    const intervalsRef = React.useRef<Record<string, number>>({})
    const telemetryRef = React.useRef(telemetry)

    React.useEffect(() => {
        telemetryRef.current = telemetry
    }, [telemetry])

    // helper: fetch ranges once up front (your existing code)
    // …

    // helper: check just one device’s last values against its ranges
    async function checkAlertsForDevice(dev) {
        const id = dev.deviceId
        const name = dev.deviceName
        const last = (arr?: string[]) => (arr?.length ? arr[arr.length - 1] : undefined)

        // fetch ranges for this one device
        // you could optimize by caching these globally instead of per call
        const [phRes, orpRes, turRes, ecRes] = await Promise.all([
            getRanges('pH'),
            getRanges('ORP'),
            getRanges('TUR'),
            getRanges('EC'),
        ])

        const rPH = phRes.data?.find(x => x.deviceId === id)
        const rORP = orpRes.data?.find(x => x.deviceId === id)
        const rTUR = turRes.data?.find(x => x.deviceId === id)
        const rEC = ecRes.data?.find(x => x.deviceId === id)

        const vPH = last(dev.pH); if (vPH !== undefined && rPH) fireAlert(+vPH, rPH.minValue, rPH.maxValue, name, id, 'pH')
        const vORP = last(dev.ORP); if (vORP !== undefined && rORP) fireAlert(+vORP, rORP.minValue, rORP.maxValue, name, id, 'ORP')
        const vTUR = last(dev.TUR); if (vTUR !== undefined && rTUR) fireAlert(+vTUR, rTUR.minValue, rTUR.maxValue, name, id, 'TUR')
        const vEC = last(dev.EC); if (vEC !== undefined && rEC) fireAlert(+vEC, rEC.minValue, rEC.maxValue, name, id, 'EC')
    }

    React.useEffect(() => {
        // 1) clear any existing timers
        Object.values(intervalsRef.current).forEach(clearInterval)
        intervalsRef.current = {}

        // 2) for each device, schedule its own timer
        telemetry.forEach((dev) => {
            // const id = dev.deviceId
            // // skip if already scheduled
            // if (intervalsRef.current[id]) return

            // // compute ms interval from dev.monitoring_time (in seconds)
            // const secs = typeof dev.monitoringTime === 'number' ? dev.monitoringTime : 10000
            // console.log('secs >>>>', secs)

            // // 2a) run once immediately
            // checkAlertsForDevice(dev)

            // // 2b) then every msRate
            // const handle = window.setInterval(() => {
            //     checkAlertsForDevice(dev)
            // }, secs)

            // intervalsRef.current[id] = handle

            const id = dev.deviceId

            const last = (arr?: string[]) => (arr?.length ? arr[arr.length - 1] : undefined)

            const secs = typeof last(dev.monitoringTime) == 'string' ? +dev.monitoringTime : 10000
            console.log('dev', dev)
            console.log('dev Arrray', dev.monitoringTime, dev.deviceId)
            console.log('dev.monitoringTime', dev.monitoringTime[0])

            // helper to fetch fresh dev data and check its alerts
            const tick = () => {
                const freshDev = telemetryRef.current.find((d) => d.deviceId === id)

                if (freshDev) {
                    checkAlertsForDevice(freshDev)
                }
            }

            // run immediately, then on interval
            tick()
            intervalsRef.current[id] = window.setInterval(tick, secs)
        })

        // 3) cleanup on unmount or device‐list change
        return () => {
            Object.values(intervalsRef.current).forEach(clearInterval)
            intervalsRef.current = {}
        }
        // only re‐run this setup when the list of deviceIds changes
    }, [telemetry.map(d => d.deviceId + ':' + (d.monitoringTime || []).join(',')).join(';')])

    return (
        <Sidebar collapsible="icon" {...props}>
            <SidebarHeader>
                <TeamSwitcher teams={data.teams} />
            </SidebarHeader>
            <SidebarContent>
                <NavMain items={data.navMain} />
                <NavProjects />
            </SidebarContent>
            <SidebarFooter>
                <NavUser user={data.user} />
            </SidebarFooter>
            <SidebarRail />
        </Sidebar>
    )
}