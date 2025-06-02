import { getRanges } from '@/actions'
import { TelemetryProvider } from '@/components/context/TelemetryProvider'
import { ReactNode } from 'react'

export default async function DashboardLayout({
    children,
    params,
}: {
    children: ReactNode
    params: { deviceId: string }
}) {

    const { data: dataPH } = await getRanges('pH', params.deviceId,)
    const { data: dataTUR } = await getRanges('TUR', params.deviceId,)
    const { data: dataORP } = await getRanges('ORP', params.deviceId,)
    const { data: dataEC } = await getRanges('EC', params.deviceId,)

    return (
        <TelemetryProvider deviceId={params.deviceId}
        // dataPH={dataPH}
        // dataTUR={dataTUR}
        // dataORP={dataORP}
        // dataEC={dataEC}
        >
            {children}
        </TelemetryProvider>
    )
}