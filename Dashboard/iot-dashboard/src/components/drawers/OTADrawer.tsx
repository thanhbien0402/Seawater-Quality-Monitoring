'use client'
import {
    Drawer,
    DrawerClose,
    DrawerContent,
    DrawerDescription,
    DrawerFooter,
    DrawerHeader,
    DrawerTitle,
    DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { ProfileForm } from '../forms/ProfileForm'
import { SquarePlus } from 'lucide-react'
import { useState } from 'react'
import { useTelemetry } from '../context/TelemetryProvider'
import { sendDataTb } from '@/actions'

export function OTADrawer({ deviceId }: { deviceId: string }) {

    const [isOpen, setIsOpen] = useState<boolean>(false)
    const token = global?.window?.localStorage.getItem('token') ?? ''


    const { currentFWVersion, fwVersion, fwTitle } = useTelemetry()

    return (
        <Drawer open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DrawerTrigger asChild>
                <Button variant="ghost">
                    <SquarePlus className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent className='h-[40vw]'>
                <DrawerHeader className="text-left">
                    <DrawerTitle>Firmware Update</DrawerTitle>
                    {(currentFWVersion === fwVersion)
                        ? `Your version ${currentFWVersion} is up-to-date`
                        : <>
                            <DrawerDescription>
                                The new version <span className='text-red-600 text font-bold'>{fwVersion}</span> is available. <span className='text-blue-700 font-bold'>Click to update!</span>
                            </DrawerDescription>
                            <Button onClick={async () => {
                                await sendDataTb(fwTitle, 'ota', token, deviceId, fwVersion)
                                setIsOpen(false)
                            }}>Update now</Button>
                        </>
                    }
                </DrawerHeader>
            </DrawerContent>
        </Drawer>
    )
}
